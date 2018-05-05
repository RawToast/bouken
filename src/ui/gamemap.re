let component = ReasonReact.statelessComponent("GameMap");

open Types;

let stateToElement = place => 
  switch place.state {
  | EMPTY => "."
  | PLAYER(_) => "O"
  | ENEMY(_) => "X"
  };

let tilesToElements =
  List.map(t =>
      switch (t.tile) {
      | GROUND => stateToElement(t)
      | WATER => "w"
      | WALL => "#" 
      }
    |> str => (" " ++ str) 
    |> ReasonReact.stringToElement);


let toeles: list(list(place)) => list(list(ReasonReact.reactElement)) = 
  (map) => map 
    |> List.map(es => es |> tilesToElements) 
    |> List.map(li => [<br/>, ...li]);

let make = (~level: level, _children) => {
  ...component,
  render: _self =>
    <div className="GameMap">
      (           
        toeles(level.map) 
          |> List.rev
          |> List.map(ts => ts |> Array.of_list |> ReasonReact.arrayToElement)
          |> Array.of_list |> ReasonReact.arrayToElement
      )
    </div>,
};