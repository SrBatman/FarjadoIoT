const { Schema, model } = require('mongoose');

const logSchema = new Schema({
    username: {
      type: String,
      required: true,
      unique: true,
    },
    sensor: {
      type: String,
      required: true,
    },
    endedDate: {
      type: String,
      required: true,
    },
  }, {
    timestamps: true,
  });

module.exports = model('log', logSchema);