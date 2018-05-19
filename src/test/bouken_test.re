open Bouken;
open Types;

open Jest;

module BasicTurnLoop = Gameloop.CreateTurnLoop(Positions.BasicPositions);
module Game = CreateGame(BasicTurnLoop);

describe("Game.Create", () => {
  
  let newGame = Game.create("dave");
  test("Creates a new game", (_) => {
    open Expect;
    
    expect(newGame.player.name) |> toBe("dave");
  });
});