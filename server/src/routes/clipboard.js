const express = require('express');
const router = express.Router();
const clipboardController = require('../controllers/clipboardController');
const { validate, clipboardItemSchema, syncPullSchema } = require('../utils/validator');
const { authMiddleware, deviceAuthMiddleware } = require('../middleware/auth');

router.use(authMiddleware);
router.use(deviceAuthMiddleware);

router.post('/push', validate(clipboardItemSchema), clipboardController.pushItem);
router.post('/push/file', clipboardController.upload.single('file'), clipboardController.pushFile);
router.get('/pull', clipboardController.pullItems);
router.get('/search', clipboardController.searchItems);
router.delete('/history', clipboardController.clearHistory);
router.get('/:itemId', clipboardController.getItem);
router.delete('/:itemId', clipboardController.deleteItem);
router.put('/:itemId/pin', clipboardController.togglePin);

module.exports = router;
