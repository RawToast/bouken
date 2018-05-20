open Bouken;
open Types;
open Rationale;

open Jest;
open Expect;

module BasicTurnLoop = Gameloop.CreateGameLoop(Positions.BasicPositions);
module Game = CreateGame(BasicTurnLoop, World.World);


describe("Game", () => {
  describe("Create", () => {
    test("Creates a new game", (_) => {
      let initGame = Game.create("dave");
      expect(initGame.player.name) |> toBe("dave");
    });
  });
});


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
  | Some(gam) => gam
  | None => initGame
  };

  test("Returns some when the move is possible", (_) => {
    expect(Option.isSome(optGame)) |> toBe(true);
  });

  test("Initially the player is located at (6, 6)", (_) => {
    expect(initGame.player.location) |> toEqual((6, 6));
  });

  test("The Player's location is updated", (_) => {
    expect(newGame.player.location) |> toEqual((5, 5));
  });
});

describe("Game.UseStairs", () => {
  let initGame = Game.create("dave");
  let newGame = initGame 
    |> Game.movePlayer(7, 8) 
    |> Option.bind(_, Game.useStairs) 
    |> Option.default(initGame);

  test("Updates player's location", (_) => {
    expect(newGame.player.location) |> toEqual((0, 9));
  });

  test("Moves the current level", (_) => {
    expect(newGame.world.current) |> toEqual("Floor 2");
  });


  test("Updates the world map", (_) => {
    let place = newGame.world |>
      World.World.currentLevel |>
        Option.bind(_, lv => 
          Level.Area.getPlace(0, 9, lv.map))
        |> Option.default({ tile: GROUND, state: EMPTY });
    
    let isPlayer = switch place.state {
    | PLAYER(_) => true;
    | _ => false;
    };

    expect(isPlayer) |> toEqual(true);
  });
});
