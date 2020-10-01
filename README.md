# flutter_secure_storage_windows

Highly experimental federated plugin for [flutter_secure_storage] plugin for Windows.

## Status

This is still in heavy development and does not secure the data right now.
Since I cannot detect for the moment which app is calling this plugin all apps will override the data of other apps.

In other words this is **just for testing** right now!

If you want to **test and _not_ use** the current implementation please add this repo as dependency (just the last 4 lines) to your `pubspec.yaml`:

```yaml
dependencies:
  flutter:
    sdk: flutter
  flutter_secure_storage: ^3.3.1+1
  # Experimental unsecure windows implementation
  flutter_secure_storage_windows:
    git:
      url: https://github.com/rekire/flutter_secure_storage_windows.git
      ref: develop
```
[flutter_secure_storage]: https://github.com/mogol/flutter_secure_storage