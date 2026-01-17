

const express = require('express')
const body_parser = require('body-parser')
const fs = require('node:fs')
const PORT = 4000
const app = express()

app.use(body_parser.json())

app.get('/get-slots', (req, res) => {
    console.log('received request')
    fs.readFile('./slots.json', (err, data) => {
        if (err)
            res.status(500).send('INTERNAL SERVER ERROR')
        else
            res.send(data)
    })
})

app.post('/post-slots', (req, res) => {
    let slots = req.body
    // remove the slots whose deleted property is true
    for ( slot in slots ) {
        if ( slots[slot].deleted )
            delete slots[slot]
    }

    fs.writeFile('./slots.json', JSON.stringify(slots), err => {
        if (err)
            res.status(500).send('INTERNAL SERVER ERROR')
        else
            res.status(201).send('SUCCESS')
    })
})

app.listen(PORT, () => {
    console.log(`Helios server listening on ${PORT}`)
})
