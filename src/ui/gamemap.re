let component = ReasonReact.statelessComponent("GameMap");

open Types;

let tilesToElements =
  List.map(t =>
      switch (t.tile) {
      | GROUND => "."
      | WATER => "w"
      | WALL => "#"
      }
    |> ReasonReact.stringToElement);

let toeles: list(list(place)) => list(list(ReasonReact.reactElement)) = (map) => 
  map |> List.map(tilesToElements);

let make = (level: level, _children) => {
  ...component,
  render: _self =>
    <div className="GameMap">
      (           
        toeles(level.map) 
          |> List.map(ts => ts |> Array.of_list |> ReasonReact.arrayToElement)
          |> Array.of_list |> ReasonReact.arrayToElement
      )
    </div>,
};