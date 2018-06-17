open Webapi.Dom;
open Types;

let component = ReasonReact.statelessComponent("GameMap");

module GameElements = {
  let makeEnemy = (e:enemy) => switch(e.name) {
  | "Zombie" => "Z"
  | "Minotaur" => "M"
  | _ => "X"
  };
  let stateToElement = (place: place, default:string) => 
  switch place.state {
  | Empty => default
  | Player(_) => "O"
  | Enemy(e) => makeEnemy(e)
  };

  let tilesToElements = List.map(t =>
    switch (t.tile) {
      | GROUND => stateToElement(t, ".")
      | WATER => stateToElement(t, "w")
      | WALL => "#"
      | STAIRS(_) => stateToElement(t, "/")
      | EXIT(_) => stateToElement(t, "e")
      }
    |> str => (" " ++ str)
    |> ReasonReact.string);
  let asElements: list(list(place)) => list(list(ReasonReact.reactElement)) =
  (map) => map
    |> List.map(es => es |> tilesToElements)
    |> List.map(li => [<br/>, ...li]);
};


let handleKeyPress = (movement, stairs, useExit, evt: Dom.keyboardEvent) => {
  evt |> KeyboardEvent.code
    |> code => switch code {
    | "KeyQ" => movement(-1, 1)
    | "KeyW" => movement(0, 1)
    | "KeyE" => movement(1, 1)
    | "KeyA" => movement(-1, 0)
    | "KeyD" => movement(1, 0)
    | "KeyZ" => movement(-1, -1)
    | "KeyX" => movement(0, -1)
    | "KeyC" => movement(1, -1)
    | "KeyS" => movement(0, 0); useExit(); stairs() /* Alt key for wait? */
    | _ => Js.Console.log("No");
    };
  ();
};

let make = (~area: area, ~movePlayer, ~takeStairs, ~useExit, _children) => {
  ...component,
  didMount: (_) =>  {
    document |> Document.addKeyDownEventListener(handleKeyPress(movePlayer, takeStairs, useExit));
  },
  render: _self =>
    <div className="GameMap">
      (
        GameElements.asElements(area)
          |> List.rev
          |> List.map(ts => ts |> Array.of_list |> ReasonReact.array)
          |> Array.of_list |> ReasonReact.array
      )
    </div>
};
