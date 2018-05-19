open Bouken;
open Types;

open Jest;
open Expect;

module BasicTurnLoop = Gameloop.CreateGameLoop(Positions.BasicPositions);
module Game = CreateGame(BasicTurnLoop, World.World);

describe("Game.Create", () => {
  
  let newGame = Game.create("dave");

  test("Creates a new game", (_) => {
    expect(newGame.player.name) |> toBe("dave");
  });
});