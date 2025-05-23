let string = React.string;

let notes =
  <div id="notes" className="c-notes">
    <div className="pre-scrollable">
      <pre className="c-messages">
        {string(
           "
O Player

. Ground

: Rough Ground

# Wall

w Shallow water (Slows all actions)

Z Enemy / Zombie

X Enemy / Generic Enemy

M Enemy / Minotaur

g Gold

+ Health

, Trap

; Snare

/ Stairs

e Exit",
         )}
      </pre>
    </div>
  </div>;

[@react.component]
let make = () => <Toggleable label="Key" content=notes />;
