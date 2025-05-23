let string = React.string;

let notes =
  <div id="notes" className="c-notes">
    <div className="pre-scrollable">
      <pre className="c-messages">
        {string(
           "
The player can move in 8 directions with:

`q` `w` `e`

`a` "
           ++ {js|🐾|js}
           ++ " `d`

`z` `x` `c`

The player can attack by moving in the direction of an enemy.

Stairs `/` can be taken in either direction using `s`.

The player can win the game using an exit `e` using `s`, but remember some exits score more points than others.

`s` will wait a turn if the player is not on stairs or an exit.",
         )}
      </pre>
    </div>
  </div>;

[@react.component]
let make = () => <Toggleable label="Guide" content=notes />;
