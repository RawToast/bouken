open ReasonReact;

let component = ReasonReact.reducerComponent("Key");
let notes = 
  <div id="notes" className="c-notes">
    <div className="pre-scrollable">
        <pre className="c-messages">
            (string("
O Player

. Ground

# Wall

w Shallow water (Slows all actions)

Z Enemy / Zombie

X Enemy / Generic Enemy

M Enemy / Minotaur

/ Stairs

e Exit")
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
      <h6 onClick=((_) => (self.send(!self.state)))>(string("Key"))</h6>
      (if (self.state) notes else ReasonReact.null)
    </div>
  }
};


