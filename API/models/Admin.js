const { Schema, model } = require('mongoose');

const adminSchema = new Schema({
    username: {
      type: String,
      required: true,
      unique: true,
    },
    email: {
      type: String,
      required: true,
      unique: true,
    },
    password: {
      type: String,
      required: true,
    },
  }, {
    timestamps: true,
  });

module.exports = model('admin', adminSchema);