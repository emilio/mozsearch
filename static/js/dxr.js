var Dxr = new (class Dxr {
  constructor() {
    let constants = document.getElementById("data");

    this.wwwRoot = constants.getAttribute("data-root");
    this.baseUrl = location.protocol + "//" + location.host;
    this.icons = this.wwwRoot + "/static/icons/";
    this.views = this.wwwRoot + "/static/templates";
    this.searchUrl = constants.getAttribute("data-search");
    this.tree = constants.getAttribute("data-tree");
    this.timeouts = {
      search: 300,
      // We start the history timeout after the search updates (i.e., after
      // timeouts.search has elapsed).
      history: 2000 - 300,
    };
    this.form = document.getElementById("basic_search");
    this.searchBox = document.getElementById("search-box");

    // TODO(emilio): Maybe these should be their own web component or
    // something.
    this.fields = {
      query: document.getElementById("query"),
      path: document.getElementById("path"),
      caseSensitive: document.getElementById("case"),
      regexp: document.getElementById("regexp"),
    };
    this.bubbles = {
      query: document.getElementById("query-bubble"),
      path: document.getElementById("path-bubble"),
    };

    this.startSearchTimer = null;
    // The timer to move to the next url.
    this.historyTimer = null;
    // The controller to allow aborting a fetch().
    this.fetchController = null;

    window.addEventListener("pageshow", () =>
      this.initFormFromLocalStorageOrUrl()
    );
    // FIXME: This reloads the page when you navigate to #lineno.
    window.addEventListener("popstate", () => window.location.reload(), {
      once: true,
    });

    this.fields.query.addEventListener("input", () => this.startSearchSoon());
    this.fields.path.addEventListener("input", () => this.startSearchSoon());
    this.fields.regexp.addEventListener("change", () => this.startSearch());
    this.fields.caseSensitive.addEventListener("change", event => {
      window.localStorage.setItem("caseSensitive", event.target.checked);
      this.startSearch();
    });
    this.initFormFromLocalStorageOrUrl();
  }

  cancel() {
    if (this.startSearchTimer) {
      clearTimeout(this.startSearchTimer);
      this.startSearchTimer = null;
    }
    if (this.historyTimer) {
      clearTimeout(this.historyTimer);
      this.historyTimer = null;
    }
    if (this.fetchController) {
      this.fetchController.abort();
      this.fetchController = null;
    }
  }

  startSearchSoon() {
    this.cancel();
    this.startSearchTimer = setTimeout(() => {
      this.startSearchTimer = null;
      this.startSearch();
    }, this.timeouts.search);
  }

  async startSearch() {
    this.cancel();

    let query = this.fields.query.value;
    let path = this.fields.path.value.trim();

    if (!query.length && !path.length) {
      return this.hideBubbles();
    }

    if (query.length < 3 && path.length < 3) {
      return this.showBubble(
        "info",
        "Enter at least 3 characters to do a search.",
        query.length ? "query" : "path"
      );
    }

    this.hideBubbles();

    let url = new URL(this.searchUrl, window.location);
    url.searchParams.set("q", this.fields.query.value);
    url.searchParams.set("path", this.fields.path.value.trim());
    url.searchParams.set("case", this.fields.caseSensitive.checked);
    url.searchParams.set("regexp", this.fields.regexp.checked);
    let controller = new AbortController();

    this.fetchController = controller;

    this.searchBox.classList.add("in-progress");
    let results;
    try {
      let response = await fetch(url.href, {
        headers: {
          Accept: "application/json",
        },
        signal: this.fetchController.signal,
      });
      results = await response.json();
      if (!response.ok) {
        return this.showBubble(results.error_level, results.error_html);
      }
    } catch (error) {
      if (controller.signal.aborted) {
        // This fetch was cancelled in order to do a new query, nothing to do
        // here.
        return;
      }
      return this.showBubble("error", "An error occurred. Please try again.");
    } finally {
      this.searchBox.classList.remove("in-progress");
    }

    populateResults(results, false, false);
    this.historyTimer = setTimeout(() => {
      this.historyTimer = null;
      window.history.pushState({}, "", url.href);
    }, this.timeouts.history);
  }

  initFormFromLocalStorageOrUrl() {
    let url = new URL(location.href);
    let params = url.searchParams;

    // If the `case` param is in the URL, returns its boolean value. Otherwise,
    // returns null.
    //
    // FIXME(emilio): Why is case sensitivity different from every other param?
    let caseSensitive = params.get("case");
    if (caseSensitive) {
      caseSensitive = caseSensitive === "true";
      // Any case-sensitivity specification in the URL overrides what was in
      // localStorage.
      window.localStorage.setItem("caseSensitive", caseSensitive);
    } else {
      caseSensitive = window.localStorage.getItem("caseSensitive") === "true";
    }
    this.fields.caseSensitive.checked = caseSensitive;

    this.fields.regexp.checked = params.get("regexp") === "true";

    let query = params.get("q");
    if (query) {
      this.fields.query.value = query;
    }

    let path = params.get("path");
    if (path) {
      this.fields.query.value = path;
    }
  }

  // Hang an advisory message off the search field.
  // @param {string} level - The seriousness: 'info', 'warning', or 'error'
  // @param {string} html - The HTML message to be displayed
  // @param {string} which - 'query' or 'path', defaults to the focused element
  // or 'query' otherwise.
  showBubble(level, html, which) {
    if (!which) {
      which = document.activeElement == this.fields.path ? "path" : "query";
    }
    let other = which == "path" ? "query" : "path";
    this.hideBubble(other);

    let bubble = this.bubbles[which];
    bubble.classList.remove("error");
    bubble.classList.remove("warning");
    bubble.classList.remove("info");
    bubble.classList.add(level);
    bubble.innerHTML = html;

    // TODO(emilio): Old code animated the bubble.
    bubble.style.display = "block";
  }

  hideBubble(which) {
    // TODO(emilio): Old code animated the bubble.
    this.bubbles[which].style.display = "none";
  }

  hideBubbles() {
    for (let kind in this.bubbles) {
      this.hideBubble(kind);
    }
  }
})();

function hashString(string) {
  let hash = 0;
  if (string.length == 0) {
    return hash;
  }
  for (let i = 0; i < string.length; i++) {
    let char = string.charCodeAt(i);
    hash = (hash << 5) - hash + char;
    hash = hash & hash; // Convert to 32bit integer
  }
  return hash;
}

function classOfResult(pathkind, qkind) {
  var klass = pathkind + ":" + qkind;
  return "EXPANDO" + hashString(klass);
}

function onExpandoClick(event) {
  var target = $(event.target);
  var open = target.hasClass("open");

  if (open) {
    $("." + target.data("klass")).hide();
    target.removeClass("open");
    target.html("&#9654;");
  } else {
    $("." + target.data("klass")).show();
    target.addClass("open");
    target.html("&#9660;");
  }
}

var contentContainer = $("#content");

var populateEpoch = 0;
function populateResults(data, full, jumpToSingle) {
  populateEpoch++;

  var title = data["*title*"];
  if (title) {
    delete data["*title*"];
    document.title = title + " - mozsearch";
  }
  var timed_out = data["*timedout*"];
  delete data["*timedout*"];

  window.scrollTo(0, 0);

  function makeURL(path) {
    return "/" + Dxr.tree + "/source/" + path;
  }

  function chooseIcon(path) {
    var suffix = path.lastIndexOf(".");
    if (suffix == -1) {
      return "unknown";
    }
    suffix = path.slice(suffix + 1);
    return (
      {
        cpp: "cpp",
        h: "h",
        c: "c",
        mm: "mm",
        js: "js",
        jsm: "js",
        py: "py",
        ini: "conf",
        sh: "sh",
        txt: "txt",
        xml: "xml",
        xul: "ui",
        java: "java",
        in: "txt",
        html: "html",
        png: "image",
        gif: "image",
        svg: "svg",
        build: "build",
        json: "js",
        css: "css",
      }[suffix] || "unknown"
    );
  }

  function renderPath(pathkind, qkind, fileResult) {
    var klass = classOfResult(pathkind, qkind);

    var html = "";
    html += "<tr class='result-head " + klass + "'>";
    html +=
      "<td class='left-column'><div class='" +
      chooseIcon(fileResult.path) +
      " icon-container'></div></td>";

    html += "<td>";

    var elts = fileResult.path.split("/");
    var pathSoFar = "";
    for (var i = 0; i < elts.length; i++) {
      if (i != 0) {
        html += "<span class='path-separator'>/</span>";
      }

      var elt = elts[i];
      pathSoFar += elt;
      html += "<a href='" + makeURL(pathSoFar) + "'>" + elt + "</a>";
      pathSoFar += "/";
    }

    html += "</td>";
    html += "</tr>";

    return html;
  }

  function renderSingleSearchResult(pathkind, qkind, file, line) {
    var [start, end] = line.bounds || [0, 0];
    var before = line.line.slice(0, start).replace(/^\s+/, "");
    var middle = line.line.slice(start, end);
    var after = line.line.slice(end).replace(/\s+$/, "");

    var klass = classOfResult(pathkind, qkind);
    var html = "";
    html += "<tr class='" + klass + "'>";
    html +=
      "<td class='left-column'><a href='" +
      makeURL(file.path) +
      "#" +
      line.lno +
      "'>" +
      line.lno +
      "</a></td>";
    html += "<td><a href='" + makeURL(file.path) + "#" + line.lno + "'>";

    function escape(s) {
      return s.replace(/&/gm, "&amp;").replace(/</gm, "&lt;");
    }

    html += "<code>";
    html += escape(before);
    html += "<b>" + escape(middle) + "</b>";
    html += escape(after);
    html += "</code>";

    html += "</a>";

    if (line.context) {
      var inside = line.context;
      if (line.contextsym) {
        var url = `/${Dxr.tree}/search?q=symbol:${encodeURIComponent(
          line.contextsym
        )}&redirect=false`;
        inside = "<a href='" + url + "'>" + line.context + "</a>";
      }
      html +=
        " <span class='result-context'>// found in <code>" +
        inside +
        "</code></span>";
    }

    html += "</td>";
    html += "</tr>";

    return html;
  }

  var count = 0;
  for (var pathkind in data) {
    for (var qkind in data[pathkind]) {
      for (var k = 0; k < data[pathkind][qkind].length; k++) {
        var path = data[pathkind][qkind][k];
        count += path.lines.length;
      }
    }
  }

  var fileCount = 0;
  for (var pathkind in data) {
    for (var qkind in data[pathkind]) {
      fileCount += data[pathkind][qkind].length;
    }
  }

  if (jumpToSingle && fileCount == 1 && count <= 1) {
    var pathkind = Object.keys(data)[0];
    var qkind = Object.keys(data[pathkind])[0];
    var file = data[pathkind][qkind][0];
    var path = file.path;

    if (count == 1) {
      var line = file.lines[0];
      var lno = line.lno;
      window.location = `/${Dxr.tree}/source/${path}#${lno}`;
    } else {
      window.location = `/${Dxr.tree}/source/${path}`;
    }
    return;
  }

  var timeoutWarning = null;
  if (timed_out) {
    timeoutWarning = $(
      `<div>Warning: results may be incomplete due to server-side search timeout!</div>`
    );
  }
  // If no data is returned, inform the user.
  if (!fileCount) {
    var user_message = contentContainer.data("no-results");
    contentContainer.empty().append($("<span>" + user_message + "</span>"));
    if (timeoutWarning) {
      container.append(timeoutWarning);
    }
  } else {
    var container = contentContainer.empty();

    if (count) {
      var numResults = $(
        `<div>Number of results: ${count} (maximum is 1000)</div>`
      );
      container.append(numResults);
    }
    if (timeoutWarning) {
      container.append(timeoutWarning);
    }

    var table = document.createElement("table");
    table.className = "results";

    container.append($(table));

    var counter = 0;

    var pathkindNames = {
      normal: null,
      test: "Test files",
      generated: "Generated code",
    };

    var html = "";
    for (var pathkind in data) {
      var pathkindName = pathkindNames[pathkind];
      if (pathkindName) {
        html += "<tr><td>&nbsp;</td></tr>";
        html +=
          "<tr><td class='section'>§</td><td><div class='result-pathkind'>" +
          pathkindName +
          "</div></td></tr>";
      }

      var qkinds = Object.keys(data[pathkind]);
      for (var qkind in data[pathkind]) {
        if (data[pathkind][qkind].length) {
          html += "<tr><td>&nbsp;</td></tr>";

          html += "<tr><td class='left-column'>";
          html +=
            "<div class='expando open' data-klass='" +
            classOfResult(pathkind, qkind) +
            "'>&#9660;</div>";
          html += "</td>";

          html += "<td><h2 class='result-kind'>" + qkind + "</h2></td></tr>";
        }

        for (var i = 0; i < data[pathkind][qkind].length; i++) {
          var file = data[pathkind][qkind][i];

          if (counter > 100 && !full) {
            break;
          }

          html += renderPath(pathkind, qkind, file);

          file.lines.map(function (line) {
            counter++;
            if (counter > 100 && !full) {
              return;
            }

            html += renderSingleSearchResult(pathkind, qkind, file, line);
          });
        }
      }
    }

    table.innerHTML = html;

    $(".expando").click(onExpandoClick);

    if (counter > 100 && !full) {
      var epoch = populateEpoch;
      setTimeout(function () {
        if (populateEpoch == epoch) {
          populateResults(data, true, false);
        }
      }, 750);
    }
  }
}

window.showSearchResults = function (results) {
  var jumpToSingle = window.location.search.indexOf("&redirect=false") == -1;
  populateResults(results, true, jumpToSingle);
};

/**
 * Adds a leading 0 to numbers less than 10 and greater that 0
 *
 * @param int number The number to test against
 *
 * return Either the original number or the number prefixed with 0
 */
function addLeadingZero(number) {
  return number <= 9 || number === 0 ? "0" + number : number;
}

/**
 * Converts string to new Date and returns a formatted date in the
 * format YYYY-MM-DD h:m
 * @param String dateString A date in string form.
 *
 */
function formatDate(dateString) {
  var fullDateTime = new Date(dateString),
    date =
      fullDateTime.getFullYear() +
      "-" +
      (fullDateTime.getMonth() + 1) +
      "-" +
      addLeadingZero(fullDateTime.getDate()),
    time =
      fullDateTime.getHours() + ":" + addLeadingZero(fullDateTime.getMinutes());

  return date + " " + time;
}

$(".pretty-date").each(function () {
  $(this).text(formatDate($(this).data("datetime")));
});

/**
 * Replace 'source' with 'raw' in href, and set that to the background-image
 */
function setBackgroundImageFromLink(anchorElement) {
  var href = anchorElement.getAttribute("href");
  // note: breaks if the tree's name is "source"
  var bg_src = href.replace("source", "raw");
  anchorElement.style.backgroundImage = "url(" + bg_src + ")";
}

window.addEventListener("load", function () {
  $(".image")
    .not(".too_fat")
    .each(function () {
      setBackgroundImageFromLink(this);
    });
});
