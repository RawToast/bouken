open Types;

open Jest;

open Expect;

module Game = Modules.Game;

describe("TurnLoop", () =>
  describe("continue", () => {
    let game = Game.create("test");
    let newGame = Modules.BasicTurnLoop.continue(game);

    test("Updates the players position", (_) =>
      expect(newGame.player.stats.position)
      |> toBeGreaterThan(game.player.stats.position)
    );

    test("Sets the player as active", (_) =>
      expect(newGame.player.stats.position) |> toBeGreaterThanOrEqual(1.)
    );
    
    test("Increments the turn", (_) =>
      expect(newGame.turn) |> toBeGreaterThan(game.turn)
    );
  })
);