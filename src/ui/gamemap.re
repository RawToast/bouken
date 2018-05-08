let component = ReasonReact.statelessComponent("GameMap");

open ReasonReact;
open Types;
open Webapi.Dom;

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


let asElements: list(list(place)) => list(list(ReasonReact.reactElement)) =
  (map) => map
    |> List.map(es => es |> tilesToElements)
    |> List.map(li => [<br/>, ...li]);


let handleKeyPress = (evt: Dom.keyboardEvent) => {
  evt |> KeyboardEvent.code
    |> code => switch code {
    | "KeyQ" => ()
    | "KeyW" => ()
    | "KeyE" => ()
    | "KeyA" => ()
    | "KeyS" => ()
    | "KeyD" => ()
    | "KeyZ" => ()
    | "KeyX" => ()
    | "KeyC" => ()
    | _ => ()
    };
  ();
};

let make = (~level: level, _children) => {
  ...component,
  didMount: (_) =>  {
    document |> Document.addKeyDownEventListener(handleKeyPress);
    
    NoUpdate;
  },
  render: _self =>
    <div className="GameMap">
      (
        asElements(level.map)
          |> List.rev
          |> List.map(ts => ts |> Array.of_list |> ReasonReact.arrayToElement)
          |> Array.of_list |> ReasonReact.arrayToElement
      )
    </div>,
};