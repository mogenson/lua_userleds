# lua_userleds
Lua module wrapper for Nuttx userleds driver

This Lua module adds an `ioctl()` method to the `file` type and defines a table of ioctl commands for the Nuttx [userleds driver](https://github.com/apache/incubator-nuttx/blob/master/include/nuttx/leds/userled.h).

## usage

To start, open the `/dev/userleds` device:
```lua
> leds = io.open("/dev/userleds", "wb")
```

The `ioctl()` method accepts a variable number of arguments and returns a variable number of results, based on the ioctl command. The global table `ioctls` contains the following commands:

### ioctls.ULEDIOC_SUPPORTED

Report the set of LEDs supported by the hardware. Returns an integer bitfield with bits set for supported LEDs.

```lua
> leds:ioctl(ioctls.ULEDIOC_SUPPORTED)
3 -- 0b11, two LEDs supported
```

### ioctls.ULEDIOC_SETLED

Set the state of one LED. Accepts an integer for LED index and boolean for LED state.

```lua
-- turn 1st LED on
> leds:ioctl(ioctls.ULEDIOC_SETLED, 0, true)
> leds:ioctl(ioctls.ULEDIOC_SETLED, 0, 1)
> leds:ioctl(ioctls.ULEDIOC_SETLED, 0, 0) -- any numeric value is true in Lua!
-- turn 2nd LED off
> leds:ioctl(ioctls.ULEDIOC_SETLED, 1, false)
> leds:ioctl(ioctls.ULEDIOC_SETLED, 1, nil)
```

### ioctls.ULEDIOC_SETALL

Set the state of all LEDs. Accepts an integer bitfield with bits set for on LEDs.

```lua
-- set 1st LED off and 2nd LED on
> leds:ioctl(ioctls.ULEDIOC_SETALL, 1 << 1)
```

### ioctls.ULEDIOC_GETALL

Get the state of all LEDs. Returns an integer bitfield with bits set for on LEDs.

```lua
> leds:ioctl(ioctls.ULEDIOC_GETALL)
2 -- 0b10, 1st LED off and 2nd LED on
```
