`wmbus_common` component
========================

This component is based on git subtree of https://github.com/wmbusmeters/wmbusmeters

In order to update the subtree, run the following command:

```bash
git subtree pull --prefix components/wmbus_common https://github.com/wmbusmeters/wmbusmeters.git REF --squash -m "Merge wmbusmeters REF"
```

Where `REF` is the branch or tag you want to pull from the `wmbusmeters` repository.

**Important:** Do not change merge commit message, as it is used to track the source of the subtree.