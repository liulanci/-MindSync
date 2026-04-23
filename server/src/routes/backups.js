const express = require('express');
const router = express.Router();
const backupController = require('../controllers/backupController');
const { validate, backupSchema } = require('../utils/validator');
const { authMiddleware } = require('../middleware/auth');

router.use(authMiddleware);

router.post('/', validate(backupSchema), backupController.createBackup);
router.get('/', backupController.getBackups);
router.post('/:backupId/restore', backupController.restoreBackup);
router.delete('/:backupId', backupController.deleteBackup);

module.exports = router;
