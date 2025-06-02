# MVSI Loader
This will transform MVS game into MVS Infinite

### Current Features:
- Anti Pak Signature Patch. Allows you to run `.pak` files with invalid `.sig` signatures.
- Anti uToc Signature Patch. Allows you to use `uToc` files (and `.uCas`) with invalid Signature Headers
- Custom Server: Allows you to change the server endpoint
- Allows you to bypass game death date
- Supports using the PopUp Dialog

### How to use:
Download the Latest MVSI version, and select the mods you want in the provided ini file. When a game updates, you might need to change the patches in the [Patterns] section. The updated patterns will be provided by me below, and in the downloaded file.
The zip file will contain 3 files. MVSI.asi, MVSI.ini, and dsound.dll. You need to place the files into your game's folder, next to tbb.dll. `MVSFolder/Binaries/Win64/`.

### All Ini Settings
You can find the [ini file](sample.ini) in this repo. It is automatically bundled in the zip file in the release.

## Download
[Download Here](https://github.com/thethiny/MVSIASI/releases)

## Credits
- Ermaccer for the [dll loader](https://github.com/ermaccer/MK1Hook/releases/)
- doubleaa (multiversuskoth) as a colaborator
- The Generic SigCheck bypass for UE5 by unknown
