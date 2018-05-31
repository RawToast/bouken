open Types;
open Jest;
open Expect;

module Positions = Positions.BasicPositions;
module BasicTurnLoop = Gameloop.CreateGameLoop(Positions, Enemy.CreateEnemyLoop(Positions, Level.Area, World.World));
module Game = Bouken.CreateGame(BasicTurnLoop, World.World, World.Builder);

describe("TurnLoop.continue", () => {
  
let game = Game.create("test");
let newGame = BasicTurnLoop.continue(game);

  test("Updates the players position", (_) => {      
    expect(newGame.player.stats.position) |> toBeGreaterThan(game.player.stats.position);
  });

  test("Sets the player as active", (_) => {  
    expect(newGame.player.stats.position) |> toBeGreaterThanOrEqual(1.);
    });
  
  test("Increments the turn", (_) => {  
    expect(newGame.turn) |> toBeGreaterThan(game.turn);
    });
});