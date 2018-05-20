[%bs.raw {|require('./app.css')|}];

open ReasonReact;
open Types;

type actions =
  | MovePlayer(int, int)
  | TakeStairs;

let component = ReasonReact.reducerComponent("App");

let movement = (x, y) => MovePlayer(x, y);

let resultToUpdate = r => switch r {
  | Some(s) => ReasonReact.Update(s)
  | None => NoUpdate
};

module BasicTurnLoop = Gameloop.CreateGameLoop(Positions.BasicPositions);
module Game = Bouken.CreateGame(BasicTurnLoop, World.World);

let currentLevel = (world) => World.World.currentLevel(world) |> Rationale.Option.default(List.hd(world.levels));

let make = (_children) => {
  ...component,
  initialState: () => Game.create("test"),
  reducer: (act, game) =>
    switch act {
    | MovePlayer(x, y) => game |> Game.movePlayer(x, y) |> resultToUpdate;
    | TakeStairs => game |> Game.useStairs |> resultToUpdate;
  },
  render: (self) =>
    <div className="App">
      <GameStats player=(self.state.player) turn=(self.state.turn) level={self.state.world.current}/>
      <GameMap 
        level=(currentLevel(self.state.world)) 
        movePlayer=(x => self.reduce((y) => MovePlayer(x, y)))
        takeStairs=(self.reduce(() => TakeStairs))
        />
    </div>
};
