# Themes and category colors

Every RA2 Node Studio theme contains a complete UI and node color palette. Switching themes replaces the palette atomically: **the Infantry color in Theme A is independent of the Infantry color in Theme B**.

## UI

Use **Edit Theme** on the toolbar or **Themes → Edit current theme**. A built-in preset is always copied rather than overwritten. You can also quickly change the selected section's *category* color using Themes → Customize selected node type.

Use the Themes menu to import and export JSON or delete custom themes. Custom themes are persisted locally via QSettings. Theme profiles are not embedded in and never modify the edited INI.

## JSON format (schema 1)

A theme JSON file is a top-level object:

- `schema`: integer 1
- `id`: optional string, ignored on import to avoid overwriting existing themes
- `name`: display name (1–80 characters)
- `ui`: map of **background, panel, surface, text, muted, border, accent, canvas** to hexadecimal #RRGGBB values
- `nodes`: map of **Infantry, Vehicle, Aircraft, Building, Weapon, Warhead, Projectile, Animation, Superweapon, Registry, Other** to hexadecimal #RRGGBB values

Export an existing profile to obtain a valid starting JSON. Unknown fields are ignored; required fields must all exist. Invalid color values are rejected with a message, and theme files greater than 128 KiB cannot be imported.

## Design constraints

- Node cards, panels, buttons, input fields, tabs, and menus use a unified square-corner (0px radius), VS-style flat desktop UI. Native Windows title bars and native system color/file dialogs may still follow the host OS window decorations.
- Built-in theme palettes are individually tuned for contrast and node-class distinction; arbitrary imported colors can override contrast, so verify readability.
- Theme editing only changes appearance, never dependency indexing, parse semantics, or INI file bytes.
