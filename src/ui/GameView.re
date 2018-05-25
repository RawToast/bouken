[%bs.raw {|require('./app.css')|}];

open ReasonReact;
open Types;

let currentLevel = (world) => World.World.currentLevel(world) |> Rationale.Option.default(List.hd(world.levels));

let component = ReasonReact.statelessComponent("GameView");

let make = (~game: game, ~movePlayer, ~takeStairs, ~useExit, _children) => {
  ...component,
  initialState: () => Home,
  render: (self) => 
    <div class="GameView">
      <GameStats player=(game.player) turn=(game.turn) level=(game.world.current) />
      <GameMap 
          level=(currentLevel(game.world)) 
          movePlayer=(movePlayer)
          takeStairs=(takeStairs)
          useExit=(useExit)
          />
    </div>
};
