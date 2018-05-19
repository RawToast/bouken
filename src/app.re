[%bs.raw {|require('./app.css')|}];

open ReasonReact;
open Types;

type actions =
  | MovePlayer(int, int);

let component = ReasonReact.reducerComponent("App");

let movement = (x, y) => MovePlayer(x, y);

let resultToUpdate = r => switch r {
  | Some(s) => ReasonReact.Update(s)
  | None => NoUpdate
};

module BasicTurnLoop = Gameloop.CreateGameLoop(Positions.BasicPositions);

module Game = Bouken.CreateGame(BasicTurnLoop);

let make = (_children) => {
  ...component,
  initialState: () => Game.create("test"),
  reducer: (act, game) =>
    switch act {
    | MovePlayer(x, y) => game |> Game.movePlayer(x, y) |> resultToUpdate;
  },
  render: (self) =>
    <div className="App">
      <GameStats player=(self.state.player) turn=(self.state.turn) />
      <GameMap level=(self.state.level) movePlayer=(x => self.reduce((y) => MovePlayer(x, y)))/>
    </div>
};
