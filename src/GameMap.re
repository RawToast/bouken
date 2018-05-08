open Webapi.Dom;
open Types;

module GameMap = {
  let component = ReasonReact.statelessComponent("GameMap");

  let stateToElement = (place: place) => 
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
  
  
  let handleKeyPress = (movement, evt: Dom.keyboardEvent) => {
    evt |> KeyboardEvent.code
      |> code => switch code {
      | "KeyQ" => movement(-1, 1)
      | "KeyW" => movement(0, 1)
      | "KeyE" => movement(1, 1)
      | "KeyA" => movement(-1, 0)
      /* | "KeyS" => movement({x: 0, y: 0}) */
      | "KeyD" => movement(1, 0)
      | "KeyZ" => movement(-1, -1)
      | "KeyX" => movement(0, -1)
      | "KeyC" => movement(1, -1)
      | _ => Js.Console.log("No");
      };
    ();
  };
  
  let make = (~level: level, ~movePlayer, _children) => {
    ...component,
    didMount: (_) =>  {
      document |> Document.addKeyDownEventListener(handleKeyPress(movePlayer));
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
  }
};