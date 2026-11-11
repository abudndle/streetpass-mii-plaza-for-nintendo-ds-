#!/usr/bin/env python3
"""Configure isolated melonDS DLDI storage without erasing unrelated settings."""
import argparse
import os
import tempfile
from pathlib import Path

DLDI_KEYS = ("DLDIEnable", "DLDIReadOnly", "DLDIFolderSync", "DLDIFolderPath")


def configure(home: Path, sd_folder: Path) -> Path:
    home = home.resolve()
    sd_folder = sd_folder.resolve()
    if home == sd_folder or home.is_relative_to(sd_folder):
        raise ValueError("Emulator home must not be inside its shared storage folder")
    sd_folder.mkdir(parents=True, exist_ok=True)
    config_dir = home / ".config" / "melonDS"
    config_dir.mkdir(parents=True, exist_ok=True)
    config = config_dir / "melonDS.ini"
    updates = {
        "DLDIEnable": "1",
        "DLDIReadOnly": "0",
        "DLDIFolderSync": "1",
        "DLDIFolderPath": str(sd_folder),
    }
    if config.is_symlink():
        raise ValueError("Refusing to modify a symlinked melonDS configuration")
    original = config.read_text(encoding="utf-8").splitlines(keepends=True) if config.exists() else []
    output = []
    seen = set()
    for line in original:
        key = line.partition("=")[0].strip()
        if key in updates:
            if key not in seen:
                output.append(f"{key}={updates[key]}\n")
                seen.add(key)
        else:
            output.append(line)
    if output and not output[-1].endswith("\n"):
        output[-1] += "\n"
    for key in DLDI_KEYS:
        if key not in seen:
            output.append(f"{key}={updates[key]}\n")
    updated = "".join(output)
    if config.exists() and config.read_text(encoding="utf-8") == updated:
        return config
    # Write in the same directory so replacement is atomic on the local filesystem.
    # Never leave a partially rewritten emulator profile after an interrupted write.
    temporary = None
    try:
        with tempfile.NamedTemporaryFile(mode="w", encoding="utf-8", dir=config_dir,
                                         prefix=".melonDS.ini.", suffix=".tmp",
                                         delete=False) as handle:
            temporary = Path(handle.name)
            handle.write(updated)
            handle.flush()
            os.fsync(handle.fileno())
        if config.exists():
            os.chmod(temporary, config.stat().st_mode & 0o777)
        os.replace(temporary, config)
    finally:
        if temporary is not None:
            temporary.unlink(missing_ok=True)
    return config


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--home", type=Path, required=True)
    parser.add_argument("--sd-folder", type=Path, required=True)
    args = parser.parse_args()
    print(configure(args.home, args.sd_folder))


if __name__ == "__main__":
    main()
