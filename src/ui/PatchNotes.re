open ReasonReact;

let component = ReasonReact.reducerComponent("PatchNotes");
let notes = 
  <div id="notes" className="c-notes">
    <div className="pre-scrollable">
        <pre className="c-messages">
            (string("
v0.2.0 New Frontend!

* New Dungeon layout
* Player vision
* Traps and gold
* Healthpacks

v0.1.0 MVP

* Initial game")
            )
        </pre>
    </div>
  </div>;

let make = ( _children) => {
  ...component,
  initialState: () => false,
  reducer: (action: bool, _ext) => ReasonReact.Update(action),
  render: (self) => {
    <div>
      <h3 onClick=((_) => (self.send(!self.state)))>(string("Patch Notes"))</h3>
      (if (self.state) notes else ReasonReact.null)
    </div>
  }
};


