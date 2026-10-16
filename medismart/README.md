# Medi Smart configuration

Use Node.js 22.9 or newer. No npm dependencies are required.

## Local setup

From this `medismart` directory, copy the template:

```powershell
Copy-Item .env.example .env
```

Fill in `.env` using the Firebase web app settings from Firebase Console > Project settings, and your ESP32 Wi-Fi settings. If `.env` already exists, edit it instead of overwriting it. Keep it outside `public/`.

Generate the website and firmware configuration:

```powershell
node scripts/configure.cjs
```

The generator creates `public/firebase-config.js` and `medismartVscode/include/local_config.h`. Both outputs and `.env` are ignored by Git. The website receives only the seven Firebase web settings; Wi-Fi credentials are written only to the firmware header. Environment variables override `.env` values for CI builds.

For the website only, use `node scripts/configure.cjs --web`. For firmware only, use `node scripts/configure.cjs --firmware`, then build/upload using PlatformIO in `medismartVscode`. Regenerate after changing settings.

Run the configuration checks with `node --test scripts/configure.test.cjs`.

## Hosting

Run Firebase commands from this `medismart` directory:

```powershell
node scripts/configure.cjs --web
firebase emulators:start --only hosting
```

To deploy:

```powershell
firebase deploy --only hosting
```

The Firebase predeploy hook generates the browser configuration automatically. For another static host, generate it before uploading the contents of `public/`. Publishing the source repository alone does not generate configuration.

## Existing GitHub secret alert

Removing a key from current files does not remove it from earlier commits or resolve an existing alert. For the Google API key reported by GitHub:

1. Review its usage and restrictions in Google Cloud Console > APIs & Services > Credentials. Create a replacement key with the required Firebase API restrictions; configure website restrictions as appropriate for your deployed domains and local development.
2. Update `FIREBASE_API_KEY` in your local `.env`, regenerate the configuration, deploy, and verify sign-in and database access.
3. Delete/revoke the exposed old key and resolve the GitHub alert with the corresponding reason. Rotate the Wi-Fi password too if the committed password is still in use.

The previous values were preserved locally during this fix; they have not been rotated. Git history has not been rewritten. Coordinate any later history cleanup with collaborators.

Firebase web configuration is visible to visitors even when generated from `.env`. Never add service-account keys, private keys, or server credentials to it. Database access must be protected by Firebase Authentication and Security Rules; this setup change does not change your deployed rules or firmware authentication.

References: [Firebase API key guidance](https://firebase.google.com/docs/projects/api-keys), [GitHub secret-scanning remediation](https://docs.github.com/en/code-security/secret-scanning/managing-alerts-from-secret-scanning/resolving-alerts).
