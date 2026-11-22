"""Write a redirect stub at the URL of every page the site no longer publishes.

The site publishes only the landing page. ``redirect_pages.tsv`` lists every
.html file the deployed documentation site served, as source path and output
path.
GitHub Pages has no server-side redirects, so after the build this hook writes a
static HTML page at each output path that points readers to the ``link`` of
the matching ``extra.legacy_pointers`` entry, resolved through
``extra.legacy_links``. Pages whose entry has a null link, or that no entry
matches, point to the landing page.

An entry matches by exact source path, or by prefix when it ends in "/"; the
first match wins. The build fails if an entry matches no listed page, names a
link missing from ``legacy_links``, or gives anything other than one link name
or null, so a typo cannot silently send readers to the wrong place. It also
fails if a stub would land on the output path of a file the build publishes.
"""

import html
import os
import posixpath

from mkdocs.exceptions import PluginError

_PAGES = os.path.join(os.path.dirname(__file__), "redirect_pages.tsv")

# Output paths of the files this build publishes. Checking these, not the site
# directory, lets ``mkdocs build --dirty`` overwrite stubs from an earlier build.
_published = set()

_STUB = """<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<title>{title}</title>
<meta http-equiv="refresh" content="0; url={url}">
<link rel="canonical" href="{url}">
<meta name="robots" content="noindex">
</head>
<body>
<p>This page moved to <a href="{url}">{title}</a>.</p>
</body>
</html>
"""


def _load_pages():
    with open(_PAGES, encoding="utf-8") as f:
        rows = [
            line.rstrip("\n").split("\t")
            for line in f
            if line.strip() and not line.startswith("#")
        ]
    bad = [row for row in rows if len(row) != 2]
    if bad:
        raise PluginError(f"redirects: malformed rows in {_PAGES}: {bad[:3]}")
    return rows


def _match(src_uri, pointers):
    for entry in pointers:
        page = entry["page"]
        if src_uri == page or (page.endswith("/") and src_uri.startswith(page)):
            return entry
    return None


def _validate(pages, links, pointers):
    uris = [src for src, _ in pages]
    errors = []
    for entry in pointers:
        page = entry["page"]
        if page.endswith("/"):
            if not any(uri.startswith(page) for uri in uris):
                errors.append(f"prefix '{page}' matches no page")
        elif page not in uris:
            errors.append(f"page '{page}' not found")
        # An entry without exactly page and link is most likely the old
        # multi-link form, whose extra links never reached a reader.
        if set(entry) != {"page", "link"}:
            errors.append(f"page '{page}' needs exactly the keys 'page' and 'link'")
            continue
        name = entry["link"]
        if name is not None and not isinstance(name, str):
            errors.append(f"page '{page}' link must be one link name or null")
        elif name is not None and name not in links:
            errors.append(f"page '{page}' names unknown link '{name}'")
    if errors:
        raise PluginError("legacy_pointers: " + "; ".join(errors))


def _target(src_uri, dest_uri, links, pointers, site_name):
    entry = _match(src_uri, pointers)
    if entry and entry["link"]:
        return links[entry["link"]]
    # Relative, so the stub works under the GitHub Pages project subpath and
    # in a local build.
    url = posixpath.relpath("index.html", posixpath.dirname(dest_uri) or ".")
    return {"title": f"the {site_name} home page", "url": url}


def on_config(config):
    _validate(
        _load_pages(),
        config.extra.get("legacy_links", {}),
        config.extra.get("legacy_pointers", []),
    )
    return config


def on_files(files, config):
    _published.clear()
    _published.update(f.dest_uri for f in files if f.inclusion.is_included())
    return files


def on_post_build(config):
    links = config.extra.get("legacy_links", {})
    pointers = config.extra.get("legacy_pointers", [])
    for src_uri, dest_uri in _load_pages():
        if dest_uri in _published:
            raise PluginError(
                f"redirects: stub for '{src_uri}' would overwrite published file '{dest_uri}'"
            )
        path = os.path.join(config.site_dir, *dest_uri.split("/"))
        target = _target(src_uri, dest_uri, links, pointers, config.site_name)
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, "w", encoding="utf-8") as f:
            f.write(
                _STUB.format(
                    title=html.escape(target["title"]), url=html.escape(target["url"])
                )
            )
