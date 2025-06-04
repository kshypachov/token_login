# Trembita Proxy Token Utility

A utility for interacting with the **Trembita 2.0** `proxy` service. It allows sending a token password into shared memory managed by the `libpasswordstore.so` library.

## 📦 Description

The utility dynamically loads the `libpasswordstore.so` library and calls the following functions:

- `LEGACY_passwordWrite` — writes the token password to shared memory.
- `LEGACY_passwordRead` — checks whether the password has been accepted by the proxy service.

If the password is incorrect, it will be wiped from shared memory by the library.

## ⚙️ Usage

```bash
./tokenutil -w <key_id> <password>
./tokenutil -r <key_id>
./tokenutil -h
```

### Arguments

| Option    | Description |
|-----------|-------------|
| `-w`      | Writes the `password` for the token identified by `key_id` to shared memory |
| `-r`      | Reads the login status for the token identified by `key_id` |
| `-h`      | Displays the help message |

### Examples

```bash
# Write password to shared memory
./tokenutil -w AlmazHSM1 my_secure_password

# Check login status
./tokenutil -r AlmazHSM1
```

## 💡 Notes

- `key_id` is the identifier of the token, visible in the Web UI of the Security Server (ШБО). It may refer to:
    - a software token;
    - an external USB HSM (Almaz, Crystal, Avtor);
    - a network HSM (Cipher HSM, Gryada).
- After using `-w`, do not immediately call `-r`. The proxy service checks shared memory periodically, and login may take some time.
- If the login is successful, the `-r` status will return "Logged in!".
- Waiting up to 60 seconds is typically sufficient for the proxy to read and use the password.

## 🛠 Dependencies

- `libpasswordstore.so` must be available at:
  ```
  /usr/share/uxp/lib/libpasswordstore.so
  ```

## 📄 License
[MIT](LICENSE) 
