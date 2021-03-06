open Types;
open Jest;
open Expect;
module Option = Belt_Option;

module Game = Modules.Game;
module GU = Modules.GameUtil;

let game = Game.create("test");

describe("GameUtil", () =>
  describe("updateLevel", () => {

    let emptyArea = [[]];

    test("Returns some", (_) => {  
        expect(GU.updateCurrentLevel(emptyArea, game) |> Option.isSome) |> toBe(true);
    });

    test("Updates the current level", (_) => {  
        let updatedLevel = 
            GU.updateCurrentLevel(emptyArea, game) 
                -> Option.flatMap(_, w => World.World.currentLevel(w)) 
                -> Option.getWithDefault(Level.LevelBuilder.makeWaterLevel("No"));

        expect(updatedLevel.map |> List.length) |> toBe(1);
    });
  })
)