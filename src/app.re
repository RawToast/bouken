[%bs.raw {|require('./app.css')|}];

[@bs.module] external logo : string = "./logo.svg";
open Level;
open ReasonReact;
open Gamemap;

type actions =
  | MovePlayer(int, int);

let component = ReasonReact.reducerComponent("App");

let initLevel = LevelBuilder.makeBlankWorld("test")
  |> Level.modifyTile(0, 0, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(0, 1, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(0, 2, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(0, 3, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(0, 4, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(0, 5, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(0, 6, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(7, 6, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(8, 6, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(9, 6, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(7, 7, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(8, 7, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(8, 8, {tile: WALL, state: EMPTY})
  |> Level.modifyTile(9, 9, {tile: GROUND, state: ENEMY({name: "spooky thing", health: 3})})
  |> Level.modifyTile(6, 6, {tile: GROUND, state: PLAYER({ name:"test", health: 10, gold: 5, location: (6, 6) })});

let movement = (x, y) => MovePlayer(x, y);

let make = (_children) => {
  ...component,
  initialState: () => initLevel,
  reducer: (act, level) =>
    switch act {
    | MovePlayer(x, y) => level |> Level.movePlayer(x, y) |> Rationale.Result.result(s => ReasonReact.Update(s), _f => NoUpdate);
  },
  render: (self) =>
    <div className="App">
      <GameMap level=(self.state) movePlayer=(x => self.reduce((y) => MovePlayer(x, y)))/>
      </div>
};
