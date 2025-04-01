const express = require('express');
const router = express.Router();
const multer = require('multer');
const upload = multer();
const Logs = require('../models/Logs');

router.get('/fire', async (req, res) => {
    const logs = await Logs.find()
    const filtrados = logs
    .filter(log => log.type === 'fuego')
    .filter(log => log.endedDate != null)
    .sort((a, b)=> b.createdAt - a.createdAt)
    ;

    
    res.json(filtrados);
    
})

router.get('/gas', async (req, res) => {
    const logs = await Logs.find()
    const filtrados = logs
    .filter(log => log.type === 'gas')
    .filter(log => log.endedDate != null)
    .sort((a, b)=> b.createdAt - a.createdAt)
    ;

    
    res.json(filtrados);
})

module.exports = router;