# Safe Graphics Checkpoint

Before changing graphics, check:

## 1. BG tile data

- [ ] `MAP_GFX_TILE_COUNT` matches `map_gfx_tiles` byte count
- [ ] byte count = `MAP_GFX_TILE_COUNT * 16`
- [ ] BG tile IDs start at `MAP_TILE_BASE`
- [ ] no OBJ sprite code uses `MAP_TILE_BASE + N`

## 2. draw_object_map()

- [ ] no out-of-range bg[] writes
- [ ] overlay x/y are within 0..15 for the current 16x16 map
- [ ] passable overlays do not change collision
- [ ] new object kind is not added together with tile count changes

## 3. init_map_sprites()

- [ ] actor sheet tile ranges do not overlap
- [ ] OAM slot table is updated
- [ ] one new OAM group at a time
- [ ] do not add camera-follow decorative sprites until fixed sprites are proven

## 4. Area switching

- [ ] town and field use separate collision/object maps
- [ ] do not mutate actor state during boot unless isolated
- [ ] hide or ignore area-inappropriate actors by area check first

## 5. White-screen debug order

If white screen happens:

```text
1. Revert latest MAP_GFX_TILE_COUNT change
2. Revert latest draw_object_map overlay
3. Revert latest init_map_sprites/OAM change
4. Check .map/.sym/.noi
5. Return to previous known-good ZIP
```
