const express = require('express');
const router = express.Router();
const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');
const Admin = require('../models/Admin');
const multer = require('multer');
const upload = multer();

const JWT_SECRET = process.env.JWT_SECRET; // ⚠️ usa dotenv en producción

// POST /api/login
router.post('/login', upload.none(), async (req, res) => {
  const { username, password } = req.body;

  console.log('Login attempt with:', username, password);

  // Verifica que los campos estén presentes
  if (!username || !password) {
    return res.status(400).json({ message: 'Usuario y contraseña son requeridos' });
  }

  try {
    // Buscar al admin por username
    const admin = await Admin.findOne({ username });
    if (!admin) {
      return res.status(401).json({ message: 'Usuario n encontrado' });
    }

    // Verificar contraseña
    const isMatch = await bcrypt.compare(password, admin.password);
    // const isMatch = logins[username] ? logins[username] === password : null
    if (!isMatch) {
      return res.status(401).json({ message: 'Contraseña incorrecta' });
    }

    // Generar token
    const token = jwt.sign({ id: admin._id }, JWT_SECRET, { expiresIn: '2h' });

    res.json({
      token,
      user: {
        id: admin._id,
        username: admin.username,
        email: admin.email,
      }
    });
  } catch (error) {
    console.error('Error en login:', error);
    res.status(500).json({ message: 'Error del servidor' });
  }
});

module.exports = router;
