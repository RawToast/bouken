open Types;
open Rationale;
open Jest;
open Expect;

module Game = Modules.Game;

describe("Game.Create", () => {
  test("Creates a new game", (_) => {
    let initGame = Game.create("dave");
    expect(initGame.player.name) |> toBe("dave");
  });
});

describe("Game.MovePlayer", () => {
  let initGame = Game.create("davey");
  let optGame = initGame |> Game.movePlayer(-1, -1);
  let newGame = optGame |> g => switch(g) {
  | Ok(gam) => gam
  | _ => initGame
  };

  test("Initially the player is located at (6, 6)", (_) => {
    expect(initGame.player.location) |> toEqual((6, 6));
  });

  describe("When the move is possible", () => {
    test("Returns some", (_) => {
      expect(Types.Operators.isOk(optGame)) |> toBe(true);
    });
  
    test("The Player's location is updated to (5,5)", (_) => {
      expect(newGame.player.location) |> toEqual((5, 5));
    });

    test("Moving increases the turn counter", () => {
      expect(newGame.turn) |> toBeGreaterThan(1.);
    });
  });
});


describe("Game.UseStairs", () => {
  open Types.Operators;

  let initGame = Game.create("dave");
  let newGame = initGame 
    |> Game.movePlayer(7, 8)
    >>= Game.useStairs
    |? initGame;

  describe("When the player is on an exit", () => {
    test("Updates player's location", (_) => {
      expect(newGame.player.location) |> toEqual((0, 9));
    });

    test("Moves the current level", (_) => {
      expect(newGame.world.current) |> toEqual("Dungeon 2");
    });

    test("Updates the world map", (_) => {
      let place = newGame.world |>
        World.World.currentLevel |>
          Option.bind(_, lv => 
            Level.Area.getPlace(0, 9, lv.map))
          |> Option.default({ tile: GROUND, state: Empty, tileEffect: NoEff, visible: false });
      
      let isPlayer = switch place.state {
      | Player(_) => true;
      | _ => false;
      };

      expect(isPlayer) |> toEqual(true);
    });
  });
});

describe("Game.ExitGame", () => {
  let game = Game.create("dave");
  let exitGame = game => game |> Game.useExit |> r => switch r {
    | EndGame(score, _) => (true, score)
    | _ => (false, 0)
    };

  describe("When the player is on an exit", () => {
    let newLevel = Level.LevelBuilder.makeBlankLevel("Dungeon 1") 
    |> Level.Level.modifyTile(6, 6, 
      { tile: EXIT(100), 
        tileEffect: NoEff,
        state: Player({name: "test", stats: { health: 10, speed: 1.0, position: 0., damage: 1 }, 
        gold: 5, 
        location: (6, 6)}),
        visible: false});

    let newWorld = World.World.updateLevel(newLevel, game.world);
    let initGame = { ... game, world: newWorld};

    let (exited, score) = exitGame(initGame);

    test("The player exits the game", (_) => {
      expect(exited) |> toBe(true);
    });

    test("A score is calculated", (_) => {
      expect(score) |> toBeGreaterThan(1);
    });
  });

  describe("When the player is not on an exit", () => {
    let newLevel = Level.LevelBuilder.makeBlankLevel("Floor 1")
      |> Level.Level.modifyTile(6, 6, 
        { tile: GROUND, 
          tileEffect: NoEff,
          state: Player({name: "test", stats: { health: 10, speed: 1.0, position: 0., damage: 1 }, 
          gold: 5, 
          location: (6, 6)}), 
          visible: false});

    let newWorld = World.World.updateLevel(newLevel, game.world);
    let initGame = { ... game, world: newWorld};

    let (exited, score) = exitGame(initGame);

    test("The player does not exit the game", (_) => {
      expect(exited) |> toBe(false);
    });

    test("No score is calculated", (_) => {
      expect(score) |> toEqual(0);
    });
  });
});
