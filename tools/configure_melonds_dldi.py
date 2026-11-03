#!/usr/bin/env python3
"""Prepare an isolated melonDS DLDI folder-sync configuration for CI smoke tests."""
import argparse
from pathlib import Path


def configure(home: Path, sd_folder: Path) -> Path:
    home = home.resolve()
    sd_folder = sd_folder.resolve()
    sd_folder.mkdir(parents=True, exist_ok=True)
    config_dir = home / ".config" / "melonDS"
    config_dir.mkdir(parents=True, exist_ok=True)
    config = config_dir / "melonDS.ini"
    # Legacy INI keys are still imported by upstream melonDS when no TOML
    # configuration exists. Folder sync maps this host directory as DLDI FAT.
    config.write_text(
        "DLDIEnable=1\n"
        "DLDIReadOnly=0\n"
        "DLDIFolderSync=1\n"
        f"DLDIFolderPath={sd_folder}\n",
        encoding="utf-8",
    )
    return config


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--home", type=Path, required=True)
    parser.add_argument("--sd-folder", type=Path, required=True)
    args = parser.parse_args()
    config = configure(args.home, args.sd_folder)
    print(f"melonDS config: {config}")
    print(f"DLDI folder-sync root: {args.sd_folder.resolve()}")
    print("DLDI folder is isolated to this CI run; no user data is accessed.")


if __name__ == "__main__":
    main()
