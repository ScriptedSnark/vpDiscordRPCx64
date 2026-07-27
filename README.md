# vpDiscordRPCx64

vpDiscordRPCx64 is unofficial plugin for [J.A.C.K](https://jack.hlfx.ru/en/), built against
[JackSDK](https://github.com/SanyaSho/JackSDK).

## Installation

 1. Download archive from [Releases](https://github.com/ScriptedSnark/vpDiscordRPCx64/releases/latest)
 2. Unpack archive
 3. Pick the directory matching your J.A.C.K release (`freeware`, `steam` or `steambeta`)
 4. If you use a 64-bit version of J.A.C.K, move `vpDiscordRPCx64.dll` to `J.A.C.K/plugins`

    If you use a 32-bit version of J.A.C.K, move `vpDiscordRPCx86.dll` to `J.A.C.K/plugins`

5. Move `vpDiscordRPC.ini` next to the plugin. It is optional, without it the built-in defaults are used.
6. That's it! Run J.A.C.K and check that Discord RPC is working.

## Settings

`vpDiscordRPC.ini` ls located next to the plugin binary and is read at load/reload time.

| Section | Key | Description |
| --- | --- | --- |
| `Discord` | `applicationId` | Discord application ID |
| `Discord` | `largeImageKey` | Asset key of large image (big icon) |
| `Discord` | `largeImageText` | Its tooltip, `{version}` expands to the editor version |
| `Discord` | `updateInterval` | Presence refresh rate in milliseconds (min. 100) |
| `Discord` | `showTimestamp` | Show elapsed timer |
| `Presence` | `idle` / `editing` / `settings` | State strings, `{map}` expands to the map being edited |

Can be configured with GUI though. Reload button in plugin actions applies changes.

## Showcase
![image1](https://user-images.githubusercontent.com/51358194/209185900-95a19b28-81ce-4a8a-99bf-f8502f3b3086.png)
![image](https://user-images.githubusercontent.com/51358194/209185885-14b804d6-353e-4cda-8662-e540221ad561.png)
![](https://github.com/user-attachments/assets/ca37b991-79fb-40e3-bfdb-2ef615763fa3)

