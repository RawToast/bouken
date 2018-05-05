[%bs.raw {|require('./app.css')|}];

[@bs.module] external logo : string = "./logo.svg";
open Level;

let component = ReasonReact.statelessComponent("App");

let level = LevelBuilder.makeBlankWorld("test")
  |> Level.modifyTile(0, 0, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(0, 1, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(0, 2, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(0, 3, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(0, 4, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(0, 5, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(0, 6, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(7, 6, {tile: WATER, state: EMPTY})
  |> Level.modifyTile(8, 6, {tile: WATER, state: EMPTY})
  |> Level.modifyTile(9, 6, {tile: WATER, state: EMPTY})
  |> Level.modifyTile(7, 7, {tile: WATER, state: EMPTY})
  |> Level.modifyTile(8, 7, {tile: WATER, state: EMPTY})
  |> Level.modifyTile(8, 8, {tile: WATER, state: EMPTY})
  |> Level.modifyTile(9, 9, {tile: WATER, state: EMPTY});

let make = (~message, _children) => {
  ...component,
  render: (_self) =>
    <div className="App">
      <GameMap level=(level)/>
    </div>
};
