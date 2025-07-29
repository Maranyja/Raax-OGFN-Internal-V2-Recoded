<h1 align="center">Raax-OGFN-Internal V2</h1>

<p align="center">
Universal Fortnite internal cheat for Season 1-19.
</p>
<p align="center">
This project is an improvement on my previous cheat, OG-Fortnite-Cheat.<br>
There are several improvements in V2, mainly performance and code quality/consistenty.<br>
If you use the cheat or find the project helpful, feel free to star the repo!
</p>

<p align="center">
	<a href="https://discord.gg/r2f8CHbrRF">Discord Server</a> |
	<a href="https://github.com/raax7/Raax-OGFN-Internal-V2/issues">Report an Issue</a> |
	<a href="https://github.com/raax7/OG-Fortnite-Cheat">V1</a>
</p>
<p align="center">
    <img alt="Stars" src="https://img.shields.io/github/stars/raax7/Raax-OGFN-Internal-V2?color=blue&style=for-the-badge">
</p>

## Table of Contents
<ol>
    <li><a href="#project-info">Project Info</a></li>
    <li><a href="#media">Media</a></li>
    <li><a href="#performance">Performance</a></li>
    <li><a href="#features">Features</a></li>
    <li><a href="#credits">Credits</a></li>
</ol>

## Project Info
This is the successor to my first universal internal cheat, [OG-Fortnite-Cheat](https://github.com/raax7/OG-Fortnite-Cheat).  

To my surprise, the original cheat became way more popular than I ever expected - the GitHub page has over 50,000 views as of now.  

But the longer it was out there, the more I started hating it. The source code is nearly unreadable, and the cheat itself is super unstable. This project fixes all those issues, serving as a complete overhaul of the original.  

Feel free to use this however you want. I'd even recommend it as a general UE4/UE5 internal base - it's much more modular and easier to work with once you get familiar with the structure.


## Media

https://www.youtube.com/watch?v=_zDw2-_H8gs


## Performance
Here you can see the performance of V2 against V1, both tested on Release_ImGui.
- Current (V2)
  - Compile Time: 8.2s
  - In game 50 players: 5-7% FPS Drop
- V1
  - Compile Time: 2m 55s
  - In game 50 players: 60% FPS Drop


## Features
<details>
  <summary>Aimbot</summary>

  - Toggle on/off
  - Bullet prediction
  - Custom keybind
  - Settings per ammo type (Shells, Light, Medium, Heavy, Other)
  - Smoothness adjustment
  - Max distance
  - Visible check
  - Sticky target
  - Show FOV
  - FOV size
  - Deadzone
  - Show deadzone FOV
  - Deadzone FOV size
  - Target selection (Distance, Degrees, Combined)
  - Target bone (Head, Neck, Chest, Pelvis, Random)
  - Random bone refresh rate
  - Show target line
</details>
<details>
  <summary>Trigger Bot</summary>

  - Toggle on/off
  - Custom keybind
  - Show FOV
  - FOV size
  - Max distance
  - Fire delay
</details>
<details>
  <summary>Visuals</summary>

  - **Player ESP**
    - Box (Full, Cornered, Full 3D)
    - Box thickness
    - Filled box with color option
    - Skeleton
    - Skeleton thickness
    - Tracer with customizable start/end points
    - Tracer thickness
    - Platform display
    - Name display
    - Current weapon display
    - Distance display
    - Max distance
    - OSI (Off Screen Indicator) with:
      - Match FOV option
      - Custom FOV
      - Size adjustment
  - **Radar**
    - Toggle on/off
    - Rotate with camera
    - Show camera FOV
    - Show guidelines
    - Max distance
    - Position adjustment (X/Y)
    - Size adjustment
    - Custom colors (background, visible, hidden)
  - **Loot**
    - Loot text with:
      - Minimum tier filter (Common to Mythic)
      - Fade off option
      - Max distance
    - Chest text with:
      - Fade off option
      - Max distance
    - Ammo box text with:
      - Fade off option
      - Max distance
    - Supply drop text with:
      - Fade off option
      - Max distance
    - Llama text with:
      - Fade off option
      - Max distance
  - **Color**
    - Primary color (visible/hidden)
    - Secondary color (visible/hidden)
</details>
<details>
  <summary>Exploits</summary>

  - **Weapon**
    - No spread with multiplier
    - No recoil with multiplier
    - No reload with time adjustment
    - Rapid fire with speed adjustment
    - Damage multiplier
    - Fast pickaxe with speed adjustment
    - Automatic weapons
  - **Player**
    - Zipline fly
    - Instant revive
    - Server name changer
</details>
<details>
  <summary>Misc</summary>

  - **Keybinds**
    - Add/remove keybinds
    - Edit existing keybinds
    - Delete all keybinds
    - Menu keybind customization
  - **Config**
    - Copy config (modified only)
    - Copy full config
    - Load config from text
    - Load default config
  - **Misc**
    - Menu keybind customization
    - Credits information
</details>

## Credits
**Main Developer**  
Raax (me)

**Contributors**  
[Toxy](https://github.com/Toxy121) - SDK improvements  
[NotTacs](https://github.com/NotTacs) - Llama & supply drop ESP  
[parkie](https://github.com/mlodyskiny) - GUI improvements  
[rikogmez](https://www.youtube.com/channel/UCh617js1hv9F247Rf4wKXhw) - Media
