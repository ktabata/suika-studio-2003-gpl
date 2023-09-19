Suika Studio (2003 version)
===========================

**Suika Studio** (西瓜 Studio, lit. "Watermelon Studio") was a small toolset to make visual novels.
It is the predecessor of Suika2.

There were few users, but this repository was published in September 2023 as a new work was released.

## The main game engine

The main game engine was influenced by the following engines:
* **AVG32** from VISUALARTS, used in "Kanon" and "AIR" (1999 and 2000 respectively)
* **AGES** from âge, used in "Rumbling Hearts" (2001)
* **ADVWin32** from F&C, used in "Canvas: Sepia-colored Motif" (2000)
* An unknown engine from NEXTON/Tactics, used in "One: Kagayaku Kisetsu e" (1998)

The main game engine was not very influenced by the following engines:
* **NScripter**
  * There were too many unintuitive instructions
* **Kirikiri2**
  * Line breaks and page breaks were not so intuitive

## The GUI

The GUI was conceived originally:
* The GUI scenario editor (Later, it became mainstream by **TyranoBuilder** in 2010s)
  * Scenarios can be converted to text scripts
  * The reverse is also possible
* The save data editor
  * This is not reimplemented in Suika2
* The visual debugger
  * Later, reimplemented as **Suika2 Pro** of Suika2

This GUI was modest but advanced.
However, the author quickly realized that editing with a GUI will reach its limits soon and is not suitable for a long story.

## Source code

From November to December 2001, Suika Studio was called **VNStudio**.
At this point, it was a Java bytecode interpreter with multimedia support.
The complete source code no longer remains.

In the spring of 2002, Suika Studio was first released:
* https://github.com/ktabata/suika-studio-2002-gpl

The source code as of the end of development in the spring 2003 is here:
* https://github.com/ktabata/suika-studio-2003-gpl

## Suika2

**Suika2** is the successor of Suika Studio.
It was completely rewritten in 2016, except the image processing part.

The following is the source code of Suika2, see how it has evolved!
* https://github.com/suika2engine/suika2
