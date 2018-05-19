open Level;
open Types;
open Rationale;

open Jest;
open Expect;

let blankWorld = LevelBuilder.makeBlankLevel("test");
let waterTile = { tile: WATER, state: EMPTY };

let initEnemy = {id: "testenemy", name: "spooky thing", stats: { health: 3, speed: 1.0, position: 0. }};
let playerAt = (x, y) => PLAYER({name:"test", stats: { health: 10, speed: 1.0, position: 0. }, gold: 5, location: (x, y)});
let nfPlayer = { name:"test", stats: { health: 10, speed: 1.0, position: 0. }, gold: 5, location: (9, 9) };

describe("Level.modify", () => {
  test("Can modify a tile", (_) => {
    let modifiedLevel =
      blankWorld
      |> Level.modifyTile(0, 0, waterTile);
    
      expect(modifiedLevel.map |> List.hd |> List.hd |> (i => i.tile)) |> toBe(WATER);
  });
});

describe("Level.findPlayer", () => {
  test("Finds the player", (_) => {
    let player =
      blankWorld
      |> Level.modifyTile(0, 0, {tile: GROUND, state: playerAt(0, 0)}) 
      |> Level.findPlayer;
    
      expect(Rationale.Option.isSome(player)) |> toEqual(true);
  });

  test("Returns empty when there is no player", (_) => {
    let player = blankWorld |> Level.findPlayer;
    
      expect(Rationale.Option.isNone(player)) |> toEqual(true);
  });
});

describe("Level.setPlayerLocation", () => {
  test("Moves the player when the destination is valid", (_) => {
    let result =
      blankWorld
      |> Level.modifyTile(0, 0, { tile: GROUND, state: playerAt(0, 0)})
      |> Level.setPlayerLocation(0, 1);
    
      expect(Result.isOk(result)) |> toEqual(true);
  });

  test("Returns an error when there is no player", (_) => {
    let result =
      Level.setPlayerLocation(0, 1, blankWorld);
    
      expect(Result.isError(result)) |> toEqual(true);
      
  });

  test("Returns an error when the target location is a wall", (_) => {
    let result =
      blankWorld
      |> Level.modifyTile(0, 0, { tile: GROUND, state: playerAt(0, 0)}) 
      |> Level.modifyTile(0, 1, { tile: WALL, state: EMPTY }) 
      |> Level.setPlayerLocation(0, 1);
    
      expect(Result.isError(result)) |> toEqual(true);
  });

  test("Moves the player can move when the target is water", (_) => {
    let result =
      blankWorld
      |> Level.modifyTile(0, 0, { tile: GROUND, state: playerAt(0, 0)})
      |> Level.modifyTile(0, 1, { tile: WATER, state: EMPTY }) 
      |> Level.setPlayerLocation(0, 1);
    
      expect(Result.isOk(result)) |> toEqual(true);
  });
});


describe("Level.movePlayer", () => {
  test("Moves the player when the destination is valid", (_) => {
    let result =
      blankWorld
      |> Level.modifyTile(0, 0, { tile: GROUND, state: playerAt(0, 0)})
      |> Level.movePlayer(0, 1);
    
      switch result {
      | Ok(level) => expect(Level.getPlace(0, 1, level.map) |> Rationale.Option.default(waterTile) |> isPlayer) |>  toEqual(true);
      | _ => failwith("Move should succeed")
      };
  });

  test("Updates the player when the destination is valid", (_) => {
    let result =
      blankWorld
      |> Level.modifyTile(0, 0, { tile: GROUND, state: playerAt(0, 0)})
      |> Level.movePlayer(0, 1);
    
      switch result {
      | Ok(level) => 
        expect(Level.findPlayer(level) |> Rationale.Option.default(nfPlayer) |> pl => pl.location) |>  toEqual((0, 1));
      | _ => failwith("Move should succeed")
      };
  });

  test("Moves the player forward twice, when requested twice", (_) => {
    let result =
      blankWorld
      |> Level.modifyTile(0, 0, { tile: GROUND, state: playerAt(0, 0)})
      |> Level.movePlayer(0, 1)
      |> Rationale.Result.getOk |> Rationale.Option.default(blankWorld)
      |> Level.movePlayer(0, 1);
    
      switch result {
      | Ok(level) => {
          let ply = Level.findPlayer(level) |> Rationale.Option.default(nfPlayer);
          
          expect(ply.location) |> toEqual((0, 2));
         };
      | _ => failwith("Move should succeed")
      };
  });
});

describe("Area", () => {
  describe("findPlayer", () => {
  test("Can find the player", (_) => {
    let player =
      blankWorld
      |> Level.modifyTile(0, 0, {tile: GROUND, state: playerAt(0, 0)}) 
      |> w => w.map |> Area.findPlayer;
    
      expect(Rationale.Option.isSome(player)) |> toEqual(true);
  });
  test("Returns none when there is no player", (_) => {
    let player =
      blankWorld
      |> w => w.map |> Area.findPlayer;
    
      expect(Rationale.Option.isSome(player)) |> toEqual(false);
  });
});

  describe("findEnemy", () => {
    test("Can find an enemy", (_) => {
      let enemy =
        blankWorld
        |> Level.modifyTile(0, 0, { tile: GROUND, state: ENEMY(initEnemy) })
        |> w => w.map |> Area.findEnemy("testenemy");
      
        expect(Rationale.Option.isSome(enemy)) |> toEqual(true);
    });
    test("Returns none when there is no enemy", (_) => {
      let enemy =
        blankWorld
        |> Level.modifyTile(0, 0, {tile: GROUND, state: ENEMY(initEnemy)}) 
        |> w => w.map |> Area.findEnemy("other enemy");
      
        expect(Rationale.Option.isSome(enemy)) |> toEqual(false);
    });
  });
});
