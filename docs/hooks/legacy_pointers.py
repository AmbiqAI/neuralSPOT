"""Resolve the per-page replacement-docs pointers declared in mkdocs.yml.

Reads ``extra.legacy_links`` (name -> {title, url}), ``extra.legacy_pointers``
(ordered list of {page, links}) and ``extra.legacy_default_links`` (links for
pages no entry matches). The build fails if an entry no longer matches any page
or a name is not in ``legacy_links``, so a renamed page cannot silently lose its
pointer.
"""

from mkdocs.exceptions import PluginError


def _match(src_uri, pointers):
    for entry in pointers:
        page = entry["page"]
        if src_uri == page or (page.endswith("/") and src_uri.startswith(page)):
            return entry
    return None


def on_files(files, config):
    links = config.extra.get("legacy_links", {})
    pointers = config.extra.get("legacy_pointers", [])
    uris = [f.src_uri for f in files.documentation_pages()]
    errors = []
    for entry in pointers:
        page = entry["page"]
        if page.endswith("/"):
            if not any(uri.startswith(page) for uri in uris):
                errors.append(f"prefix '{page}' matches no page")
        elif page not in uris:
            errors.append(f"page '{page}' not found")
        errors.extend(
            f"page '{page}' names unknown link '{name}'"
            for name in entry.get("links", [])
            if name not in links
        )
    errors.extend(
        f"legacy_default_links names unknown link '{name}'"
        for name in config.extra.get("legacy_default_links", [])
        if name not in links
    )
    if errors:
        raise PluginError("legacy_pointers: " + "; ".join(errors))
    return files


def on_page_context(context, page, config, nav):
    links = config.extra.get("legacy_links", {})
    entry = _match(page.file.src_uri, config.extra.get("legacy_pointers", []))
    names = entry["links"] if entry else config.extra.get("legacy_default_links", [])
    context["legacy_links"] = [links[name] for name in names]
    return context
