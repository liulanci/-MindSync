const express = require('express');
const router = express.Router();
const deviceController = require('../controllers/deviceController');
const { validate, deviceAuthSchema } = require('../utils/validator');
const { authMiddleware } = require('../middleware/auth');

router.use(authMiddleware);

router.post('/', validate(deviceAuthSchema), deviceController.registerDevice);
router.get('/', deviceController.getDevices);
router.put('/:deviceId/authorize', deviceController.authorizeDevice);
router.delete('/:deviceId/authorize', deviceController.revokeDevice);

module.exports = router;
