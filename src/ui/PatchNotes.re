let string = React.string;

let notes =
  <div id="notes" className="c-notes">
    <div className="pre-scrollable">
      <pre className="c-messages">
        {string(
           "
v0.3.0c

* Lots of technology updates, no changes to the actual game

v0.3.0b

* Initial Tile theming and colours "
           ++ {js|🙀|js}
           ++ "

v0.2.1

* Enemy vision and memory
* Additional `tall grass` tile
* Coloured water tiles

v0.2.0 New Frontend!

* New Dungeon layout
* Player vision
* Traps and gold
* Healthpacks

v0.1.0 MVP

* Initial game",
         )}
      </pre>
    </div>
  </div>;

[@react.component]
let make = () => <Toggleable label="Patch Notes" content=notes />;
