# Robot Hardware, CAD & Electronics

This directory contains the full mechanical design assets and electronic manufacturing data required to fabricate and assemble the physical self-balancing robot.

---

## 1. 3D CAD & Mechanical Design (`/cad`)

The structural chassis parts are modeled in SolidWorks. Print-ready formats and raw design models are split by intent:

* **3D Printing Files (`/3d_prints`):** Contains **`.3mf` format files** ready to import into your slicer (Bambu Studio, PrusaSlicer, OrcaSlicer, etc.).
  * *Includes parts:* BackPiece, FrontFacePiece, InnerChassis, LightDiffuser, MiddleHeadCover, TopShelf, WheelRim, and WheelTread.

* **Source Models (`/source`):** 
  * `solidwork_files/` - Raw project design history files for modifications.
  * `step_files/` - Universal solid formats to import into alternate CAD software (Fusion 360, Onshape, Inventor, etc.).

---

## 2. Electronics & Custom Controller (`/electronics`)

This layout contains the electrical schematic routing and custom PCB trace blueprints for the core onboard robot controller.

* **Fabrication Packages (`/fabrication`):**
  * `gerbers.zip` - Production data package ready to drop directly into production engines (JLCPCB)
  * `schematic.pdf` - Document for trace validation and testing diagnostics.

* **Source Blueprints (`/source`):**
  * Contains the editable **KiCad project project assets** (`.kicad_pcb`, `.kicad_sch`) alongside custom component symbols utilized for the board design routing layout.