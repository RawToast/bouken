open Jest;
open Level;
open Types;

test("Can modify a tile", (_) => {
  open Expect;
  let modifiedLevel =
    LevelBuilder.makeBlankWorld("test") 
     |> Level.modifyTile(0, 0, {tile: WATER, items: [], enemies: []});
  
    expect(modifiedLevel.map |> List.hd |> List.hd |> (i => i.tile)) |> toBe(WATER);
});