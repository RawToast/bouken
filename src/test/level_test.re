open Level;
open Types;
open Rationale;

open Jest;
open Expect;

let blankWorld = LevelBuilder.makeBlankLevel("test");
let waterTile = { tile: WATER, state: Empty, tileEffect: NoEff, visible: false };
let initialPosition = 1.;
let initEnemy = Enemy.Enemies.makeEnemy();
let playerAt = (x, y) => Player({name:"test", stats: { health: 10, speed: 1.0, position: initialPosition, damage: 3 }, gold: 5, location: (x, y)});
let nfPlayer = { name:"test", stats: { health: 10, speed: 1.0, position: initialPosition, damage: 3 }, gold: 5, location: (9, 9) };

describe("Level.modify", () => {
  test("Can modify a tile", (_) => {
    let modifiedLevel =
      blankWorld
      |> Level.modifyTile(0, 0, waterTile);
    
      expect(modifiedLevel.map |> List.hd |> List.hd |> (i => i.tile)) |> toBe(WATER);
  });
});

describe("Area", () => {
  describe("findPlayer", () => {
    test("Can find the player", (_) => {
      let player =
        blankWorld
        |> Level.modifyTile(0, 0, {tile: GROUND, state: playerAt(0, 0), tileEffect: NoEff, visible: false }) 
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
        |> Level.modifyTile(0, 0, { tile: GROUND, state: Enemy({ ... initEnemy, id: "Enemy" }), tileEffect: NoEff, visible: false })
        |> w => w.map |> Area.findEnemy("Enemy");
      
        expect(Rationale.Option.isSome(enemy)) |> toEqual(true);
    });
    test("Returns none when there is no enemy", (_) => {
      let enemy =
        blankWorld
        |> Level.modifyTile(0, 0, {tile: GROUND, state: Enemy(initEnemy), tileEffect: NoEff, visible: false }) 
        |> w => w.map |> Area.findEnemy("other enemy");
      
        expect(Rationale.Option.isSome(enemy)) |> toEqual(false);
    });
  });


  describe("movePlayer", () => {
    describe("When the Player is on a ground tile", () => {
      let areaResult =
          blankWorld
            |> Level.modifyTile(2, 2, {tile: GROUND, state: playerAt(2, 2), tileEffect: NoEff, visible: false })
            |> level => level.map
            |> Area.movePlayer(1, 0, 1.)
            |> Rationale.Option.ofResult;

      let playerArea = areaResult |> Rationale.Option.default({ player: nfPlayer, area: [[]]});

      test("The move is successful", (_) => {  
        expect(Rationale.Option.isSome(areaResult)) |> toEqual(true);
      });

      test("The player still exists", (_) => {  
        expect(playerArea.area |> Area.findPlayer |> Option.isSome) |> toBe(true);
      });

      test("The player is updated ", (_) => {  
        expect(playerArea.area |> Area.findPlayer |> Option.default(nfPlayer) |> p => p.stats.position) |> toBeCloseTo(0.);
      });
    });

    describe("When the Player is on a water tile", () => {
      let areaResult =
          LevelBuilder.makeWaterLevel("waterLevel")
            |> Level.modifyTile(2, 2, {tile: WATER, state: playerAt(2, 2), tileEffect: NoEff, visible: false })
            |> level => level.map
            |> Area.movePlayer(1, 0, 1.)
            |> Rationale.Option.ofResult;

      let playerArea = areaResult |> Rationale.Option.default({ player: nfPlayer, area: [[]]});

      test("The move is successful", (_) => {  
        expect(Rationale.Option.isSome(areaResult)) |> toEqual(true);
      });

      test("The player still exists", (_) => {  
        expect(playerArea.area |> Area.findPlayer |> Option.isSome) |> toBe(true);
      });

      test("The player in the area updated ", (_) => {  
        expect(playerArea.area |> Area.findPlayer |> Option.default(nfPlayer) |> p => p.stats.position) |> toBeCloseTo(-0.5);
      });
    });
  });
});
