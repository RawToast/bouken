open Types;
open ReasonReact;
let component = ReasonReact.statelessComponent("GameMap");

module GameElements = {
  let makeEnemy = (e:enemy) => switch(e.name) {
  | "Zombie" => ("Z", "enemy")
  | "Minotaur" => ("M", "enemy")
  | _ => ("X", "enemy")
  };

  let makeObject = (t, default) => switch(t.tileEffect) {
      | Trap(_) => (":", "trap")
      | Snare(_) => (";", "trap")
      | Heal(_) => ("+", "health")
      | Gold(_) => ("g", "gold")
      | NoEff => default
      };
    
  let stateToElement = (place: place, default) => 
    switch place.state {
    | Empty => default
    | Player(_) => ("O", "player")
    | Enemy(e) => makeEnemy(e)
    };

  let tilesToElements = (index, places) => places |> List.mapi((i, t) =>
    switch (t.tile) {
      | GROUND => makeObject(t, (".", "ground")) |> stateToElement(t)
      | WATER => makeObject(t, ("w", "water")) |> stateToElement(t)
      | WALL => ("#", "wall") |> stateToElement(t)
      | STAIRS(_) => makeObject(t, ("/", "stairs")) |> stateToElement(t)
      | EXIT(_) => makeObject(t, ("e", "exit")) |> stateToElement(t)
      }
    |> ((str, clazz)) => (" " ++ str, clazz)
    |> ((str, clazz)) => (<text key=(string_of_int(index)++"x"++string_of_int(i)) className=("map-" ++ clazz)>(string(str))</text>));
  
  let asElements: list(list(place)) => list(list(ReasonReact.reactElement)) =
  (map) => map
    |> List.mapi((i, es) => es |> tilesToElements(i))
    |> List.map(li => [<br/>, ...li]);
};


let handleKeyPress = (movement, stairs, useExit, evt: Dom.keyboardEvent) => {
  evt |> Webapi.Dom.KeyboardEvent.code
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

open Webapi.Dom;

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
