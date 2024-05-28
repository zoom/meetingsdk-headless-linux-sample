import zoomSdk from '@zoom/appssdk';
import {marked} from "marked";
import socket from './lib/socket.js';

const onUpdate = ({ transcript, sentiment}) => {
    let transcriptEl = document.getElementById('transcript');
    let sentimentEl = document.getElementById('sentiment');


    if (transcript)
        transcriptEl.innerHTML = transcript;

    if (sentiment)
        sentimentEl.innerHTML = marked.parse(sentiment)
}

(async ()=>{
    try {
        const configResponse = await zoomSdk.config({
            capabilities: [ 'getMeetingUUID' ],
        });

        console.debug('Zoom JS SDK Configuration', configResponse);

        const { meetingUUID } = await zoomSdk.getMeetingUUID();

        socket.on('update', onUpdate);
        socket.emit('join', { meetingUUID });

    } catch (e) {
        console.error(e);
    }
})();

