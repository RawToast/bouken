// open ReasonReact;

// let component = ReasonReact.reducerComponent("Key");
// let notes = 
//   <div id="notes" className="c-notes">
//     <div className="pre-scrollable">
//         <pre className="c-messages">
//             (string("
// O Player

// . Ground

// : Rough Ground

// # Wall

// w Shallow water (Slows all actions)

// Z Enemy / Zombie

// X Enemy / Generic Enemy

// M Enemy / Minotaur

// g Gold

// + Health

// , Trap

// ; Snare

// / Stairs

// e Exit")
//             )
//         </pre>
//     </div>
//   </div>;

// let make = ( _children) => {
//   ...component,
//   initialState: () => false,
//   reducer: (action: bool, _ext) => ReasonReact.Update(action),
//   render: (self) => {
//     <div>
//       <h3 onClick=((_) => (self.send(!self.state)))>(string("Key"))</h3>
//       (if (self.state) notes else ReasonReact.null)
//     </div>
//   }
// };


